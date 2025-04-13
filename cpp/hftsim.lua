require "bundle"
function main()
          require "hftsim"
          local glerp, glerp_2f,
          glerp_3f, glerp_4f,
          glerp_mat4f = Jmath.GetLerps()

          local hft   = Engine.GameFramework({
                    validation = true,
                    bc = vec4(0, 0, 0, 1)
          })
          local data  = {
                    mat4(vec4(2, 3, 0, 0), vec4(1, 0, 0, 1), vec4(0), vec4(0)),
                    mat4(vec4(1, 3, 0, 0), vec4(0, 1, 0, 1), vec4(0), vec4(0)),
                    mat4(vec4(2, 2, 0, 0), vec4(0, 0, 1, 1), vec4(0), vec4(0)), }
          --          hft.scale = hft.wr.
          -- local text   = hft.wr.CreateTextLabel(vec3(100, 100, 50), vec3(1), hft.nf, "FUCK", vec4(1))
          local graph = hft.PointGraph(vec3(100, 100, 1), vec3(500, 500, 1))
          hft.wr.AnimationGeneralPush(function(inT)
                    local vec = glerp_3f(vec3(2), vec3(10), inT)
                    graph.Update(vec3(100, 100, 1), vec3(500, 500, 1), data, {
                              from = vec3(0),
                              to = vec
                    })
          end, 0.1)
          hft.Update   = function()
                    -- print("MAJOR Update")
                    hft.wr:Update()
          end

          hft.Dispatch = function()
                    hft.wr:Dispatch()
          end

          hft.Draw     = function()
                    hft.wr:Draw()
          end

          hft.loop()
end
