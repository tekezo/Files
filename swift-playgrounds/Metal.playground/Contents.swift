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

    float2 p = float2(gid) / float2(width, height);
    p -= 0.5;
    p.x *= min(width / height, 1.0);
    p.y *= min(height / width, 1.0);

    float r = time * 0.5 + 0.8;
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
    private var color: vector_float3 = vector_float3(0.3, 0.2, 1.0) // rgb
    public init?(mtkView: MTKView) {
        view = mtkView
        device = MTLCreateSystemDefaultDevice()!
        commandQueue = device.makeCommandQueue()
        let library = try! device.makeLibrary(source: source, options: nil)
        let function = library.makeFunction(name: "focusedWindowChangedEffect")!
        cps = try! device.makeComputePipelineState(function: function)

        super.init()
        view.delegate = self
        view.device = device
    }

    public func mtkView(_: MTKView, drawableSizeWillChange _: CGSize) {}

    public func draw(in view: MTKView) {
        var time = Float(Date().timeIntervalSince(startDate))

        if time > 1.0 {
            view.isPaused = true
        }

        if let drawable = view.currentDrawable,
            let commandBuffer = commandQueue.makeCommandBuffer(),
            let commandEncoder = commandBuffer.makeComputeCommandEncoder() {
            commandEncoder.setComputePipelineState(cps)
            commandEncoder.setTexture(drawable.texture, index: 0)

            commandEncoder.setBytes(&time, length: MemoryLayout<Float>.size * 1, index: 0)
            commandEncoder.setBytes(&color, length: MemoryLayout<Float>.size * 3, index: 1)

            let w = cps.threadExecutionWidth
            let h = cps.maxTotalThreadsPerThreadgroup / w
            let threadsPerThreadgroup = MTLSize(width: w, height: h, depth: 1)
            let threadsPerGrid = MTLSize(width: drawable.texture.width,
                                         height: drawable.texture.height,
                                         depth: 1)
            commandEncoder.dispatchThreads(threadsPerGrid,
                                           threadsPerThreadgroup: threadsPerThreadgroup)
            commandEncoder.endEncoding()
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }
    }
    
    func restart() {
        startDate = Date()
        view.isPaused = false
    }
    
    func setColor(_ c: vector_float3) {
        color = c
        restart()
    }
}

struct RepresentedMetalView: NSViewRepresentable {
    let view : MTKView;
    
    func makeNSView(context: Context) -> MTKView {
        return view
    }
    
    func updateNSView(_ view: MTKView, context: Context) {
    }
}

let view = MTKView()
let delegate = MetalView(mtkView: view)
view.delegate = delegate
//PlaygroundPage.current.liveView = view

struct ContentView: View {
    var body: some View {
        VStack {
            Text("SwiftUI + Metal")
            RepresentedMetalView(view: view).frame(
                minWidth: 200,
                minHeight: 200
            )
            Button(action:{
                delegate?.restart()
            }) {
                Text("Restart")
            }
            
            HStack {
                Button(action:{
                    delegate?.setColor(vector_float3(1.0, 0.3, 0.2))
                }) {
                    Text("Red")
                }
                Button(action:{
                    delegate?.setColor(vector_float3(0.2, 1.0, 0.3))
                }) {
                    Text("Green")
                }
                Button(action:{
                    delegate?.setColor(vector_float3(0.3, 0.2, 1.0))
                }) {
                    Text("Blue")
                }
            }
        }
    }
}
PlaygroundPage.current.setLiveView(ContentView())
