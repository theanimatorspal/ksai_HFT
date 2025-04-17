require "bundle"
function main()
          require "hftsim"

          local market = hft.market()
          Engine.log(market)

          local glerp, glerp_2f,
          glerp_3f, glerp_4f,
          glerp_mat4f  = Jmath.GetLerps()

          local hft    = Engine.GameFramework({
                    validation = false,
                    bc = vec4(0, 0, 0, 1)
          })
          local data   = {
                    mat4(vec4(2, 3, 0, 0), vec4(1, 0, 0, 1), vec4(0), vec4(0)),
                    mat4(vec4(1, 3, 0, 0), vec4(0, 1, 0, 1), vec4(0), vec4(0)),
                    mat4(vec4(2, 2, 0, 0), vec4(0, 0, 1, 1), vec4(0), vec4(0)), }
          --          hft.scale = hft.wr.

          local graph  =
              hft.Graph()
              .Config()
              .Scale()
              .Points(data)

          -- hft.wr.AnimationGeneralPush(function(inT)
          --           local vec = glerp_3f(vec3(2), vec3(10), inT)
          --           graph.Config {
          --                     from = vec3(0),
          --                     to = vec
          --           }
          --           graph.Points(data)
          -- end, 0.1)

          -- hft.wr.AnimationGeneralPush(function(inT)
          --           local vec = glerp_3f(vec3(2), vec3(10), inT)
          --           graph.Config {
          --                     from = vec3(0),
          --                     to = vec
          --           }
          --           graph.Scale()
          -- end, 0.1)

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
