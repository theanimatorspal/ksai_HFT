require "bundle"
function main()
          require "hftsim"
          local hft         = Engine.GameFramework({
                    validation = true,
                    bc = vec4(0, 0, 0, 1)
          })
          --          hft.scale = hft.wr.
          -- local text   = hft.wr.CreateTextLabel(vec3(100, 100, 50), vec3(1), hft.nf, "FUCK", vec4(1))
          local debug_image = hft.I(vec3(100, 100, 1), vec3(500, 500, 1))
          local scale       = hft.wr.CreateScale2D(
                    vec3(100, 100, 1),
                    vec3(500, 500, 1),
                    hft.sf,
                    vec3(15, 15, 5),
                    vec3(0, 0, 0),
                    vec3(4, 4, 4),
                    "%.2f",
                    "%.2f",
                    vec4(1, 0, 0, 1)
          )
          hft.Update        = function()
                    hft.wr:Update()
          end

          hft.Dispatch      = function()
                    hft.wr:Dispatch()
                    debug_image.DrawDebugLines()
          end

          hft.Draw          = function()
                    hft.wr:Draw()
          end

          hft.loop()
end
