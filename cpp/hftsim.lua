require "bundle"
function main()
    require "hftsim"
    Jkr.GetLayoutsAsVH()
    local glerp, glerp_2f, glerp_3f, glerp_4f, glerp_mat4f = Jmath.GetLerps()


    local app             = Engine.GameFramework({
        validation = true,
        bc = vec4(0, 0, 0, 1),
        colors = {
            b = vec4(0, 0.9, 0.9, 0.5),
            t = vec4(0, 0, 0, 1),
            tb = vec4(0, 0.9, 0.9, 1)
        },
        wd = vec2(1500, 800),
        fd = vec2(1500, 800)
    })

    local c               = app.colors

    local Window          = function(title, c1, c2)
        return app.wr.CreateWindowScissor(
            vec3(100, 100, 1),
            vec3(100, 100, 1),
            app.nf,
            title, c1, c2, c2 * 0.5, nil, true)
    end

    local market_window   = Window("Market", c.t, vec4(1, 0.5, 0.5, 1))
    local controls_window = Window("Controls", c.t, vec4(0.5, 1, 0.5, 1))
    local data_window     = Window("Data", c.t, vec4(0.5, 0.5, 1, 1))

    local data            = {
        mat4(vec4(2, 3, 0, 0), vec4(1, 0, 0, 1), vec4(0), vec4(0)),
        mat4(vec4(1, 3, 0, 0), vec4(0, 1, 0, 1), vec4(0), vec4(0)),
        mat4(vec4(2, 2, 0, 0), vec4(0, 0, 1, 1), vec4(0), vec4(0)), }

    --window.SetCentralComponent(graph)
    -- window:Update(vec3(100, 100, 50), vec3(100, 100, 1))

    local B               = app.B
    local E               = B { e = true }

    local graph           = app.Graph()
    local graph_padding   = app.Pad(graph)

    app.Graph()
    market_window.SetCentralComponent(graph_padding)

    local layout = H(
        {
            V(
                {
                    market_window,
                    E,
                    controls_window,
                },
                { 0.7, 0.001, 0.3 }
            ),
            E,
            data_window
        }, { 0.7, 0.001, 0.3 })

    layout:Update(vec3(0, 0, app.bd), vec3(app.fd.x, app.fd.y, 1))

    graph.Config().Scale().Points(data)

    -- app.wr.AnimationGeneralPush(function(inT)
    --     local vec = glerp_3f(vec3(2), vec3(10), inT)
    --     graph.Config {
    --         from = vec3(0),
    --         to = vec
    --     }.Clear().Points(data).Scale()
    -- end, 0.1)

    app.Update   = function()
        -- print("MAJOR Update")
        app.wr:Update()
    end

    app.Dispatch = function()
        app.wr:Dispatch()
    end

    app.Draw     = function()
        app.wr:Draw()
    end

    app.loop()
end
