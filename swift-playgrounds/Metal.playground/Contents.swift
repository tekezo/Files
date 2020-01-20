import Foundation
import MetalKit
import PlaygroundSupport
import SwiftUI

let source = """
#include <metal_stdlib>
using namespace metal;

float3 circle(float2 uv, float r, float blur, float3 color) {
    float d = length(uv);
    float c = smoothstep(r, r - blur, d);
    return float3(color * c);
}

kernel void focusedWindowChangedEffect(texture2d<float, access::write> o[[texture(0)]],
                                       constant float &time [[buffer(0)]],
                                       constant float3 &color [[buffer(1)]],
                                       ushort2 gid [[thread_position_in_grid]]) {
    float width = o.get_width();
    float height = o.get_height();

    float2 uv, p = float2(gid) / float2(width, height);
    p -= 0.5;
    p.x *= max(width / height, 1.0);
    p.y *= max(height / width, 1.0);

    float r = time * 2.0 + 0.5;
    float ir = clamp(-1.0 * time + 1.0, 0.4, 1.0);

    float3 c = circle(p, r, 0.04, color);
    c -= circle(p, r, ir, color);

    o.write(float4(c, 1.0), gid);
}
"""

public class MetalView: NSObject, MTKViewDelegate {
    weak var view: MTKView!
    let commandQueue: MTLCommandQueue!
    let device: MTLDevice!
    let cps: MTLComputePipelineState!
    private var startDate: Date = Date()
    
    public init?(mtkView: MTKView) {
        view = mtkView
        view.clearColor = MTLClearColorMake(0.5, 0.5, 0.5, 1)
        view.colorPixelFormat = .bgra8Unorm
        device = MTLCreateSystemDefaultDevice()!
        commandQueue = device.makeCommandQueue()
        let library = try! device.makeLibrary(source: source, options: nil)
        let function = library.makeFunction(name:"focusedWindowChangedEffect")!
        cps = try! device.makeComputePipelineState(function: function)
        
        super.init()
        view.delegate = self
        view.device = device
    }
    
    public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}
    
    public func draw(in view: MTKView) {
        var time = Float(Date().timeIntervalSince(startDate))
        
        if (time > 1) {
            startDate = Date()
        }
        
        var color = vector_float3(0.3, 0.2, 1.0) // rgba

        if let drawable = view.currentDrawable,
            let commandBuffer = commandQueue.makeCommandBuffer(),
            let commandEncoder = commandBuffer.makeComputeCommandEncoder() {
            commandEncoder.setComputePipelineState(cps)
            commandEncoder.setTexture(drawable.texture, index: 0)

            commandEncoder.setBytes(&time, length: MemoryLayout<Float>.size * 1, index: 0)
            commandEncoder.setBytes(&color, length: MemoryLayout<Float>.size * 3, index: 1)

            let w = cps.threadExecutionWidth
            let h = cps.maxTotalThreadsPerThreadgroup / w;
            let threadsPerThreadgroup = MTLSize(width: w, height: h, depth: 1)
            let threadsPerGrid = MTLSize(width: Int(view.frame.width) * 2,
                                         height: Int(view.frame.height) * 2,
                                         depth: 1);
            commandEncoder.dispatchThreads(threadsPerGrid,
                                           threadsPerThreadgroup:threadsPerThreadgroup)
            commandEncoder.endEncoding()
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }
    }
}

let frame = CGRect(x: 0, y: 0, width: 400, height: 600)
let view = MTKView(frame: frame)
let delegate = MetalView(mtkView: view)
view.delegate = delegate
PlaygroundPage.current.liveView = view

