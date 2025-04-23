require "bundle"
function main()
    require "hftsim"

    local lerp = Jmath.GetLerps()
    local app = Engine.GameFramework({
        validation = true,
        bc = vec4(0, 0, 0, 1),
        wd = vec2(1500, 800),
        fd = vec2(1500, 800)
    })
    app.w:Maximize()

    --- SIMULATION
    local market = hft.market()
    table.Operate(
        string.ParseCSV(
            file.ReadAsString("stocks.csv")
        ),
        function(el)
            market:add_stock(el[1], tonumber(el[2]))
        end, 2)

    table.Operate(
        string.ParseCSV(
            file.ReadAsString("traders.csv")
        ),
        function(el)
            market:add_trader(math.floor(tonumber(el[1])), tonumber(el[2]))
        end, 2)

    --- UI
    do
        local c      = {
            b = vec4(0, 0.9, 0.9, 0.5),
            t = vec4(0, 0, 0, 1),
            tb = vec4(0, 0.9, 0.9, 1)
        }
        local B      = app.B
        local E      = B { e = true }
        local Window = function(title, c1, c2)
            return app.wr.CreateWindowScissor(
                vec3(100, 100, 1),
                vec3(100, 100, 1),
                app.nf,
                title, c1, c2, c2 * 0.5, nil, true)
        end
        local PB     = function(en, intit, infu)
            return app.B { onclick = infu, t = intit, en = en }
        end


        local ui_market_window   = Window("Market", c.t, vec4(1, 0.5, 0.5, 1))
        local ui_controls_window = Window("Controls", c.t, vec4(0.5, 1, 0.5, 1))
        local ui_data_window     = Window("Data", c.t, vec4(0.5, 0.5, 1, 1))
        local ui_graph           = app.Graph()
        local ui_graph_padding   = app.Pad(ui_graph)
        ui_market_window.SetCentralComponent(ui_graph_padding)

        local ui_table = app.wr.CreateDisplayTable(
            vec3(0, 0, -10),
            vec3(100, 100, 1),
            function(row, column, text)
                return B
                    {
                        t = text,
                        en = row .. "__" .. column .. "ui_table",
                    }
            end
        )
        local ui_table_padding = app.Pad(ui_table, 0.1, 0.1)
        local ui_layout = H(
            {
                V(
                    {
                        --@note Market Window
                        --============================
                        ui_market_window,


                        --============================
                        E,

                        --@note Control Window
                        --============================
                        S { ui_controls_window,
                            V(
                                {
                                    E,
                                    E,
                                    E,
                                    E,
                                    H(
                                        {
                                            E,
                                            E,
                                            E,
                                            B { t = "run" },
                                        }, CR(4)
                                    ),
                                }, CR(5))
                        },
                        --============================
                    },
                    { 0.7, 0.001, 0.3 }
                ),
                E,

                -- @note Data Window
                --============================
                S {
                    ui_data_window,
                    ui_table_padding,
                }
                --============================
            }, { 0.7, 0.001, 0.3 })

        ui_layout:Update(vec3(0, 0, app.bd), vec3(app.fd.x, app.fd.y, 1))
        ui_graph.Config({ from = vec3(0), to = vec3(20) }).Scale().Points {}

        app.Update   = function()
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
end
