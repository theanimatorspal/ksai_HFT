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
            end, 10
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
                                    E,
                                    H(
                                        {
                                            E,
                                            B {
                                                t = "run",
                                                onclick = function()
                                                    local bb = app.els["run_stop_button"]
                                                    if type(app.market_running) == "nil" then
                                                        market:run()
                                                        app.market_running = true
                                                        bb:Update(bb.mP, bb.mD, app.nf, "pause")
                                                    elseif app.market_running == true then
                                                        market:pause()
                                                        bb:Update(bb.mP, bb.mD, app.nf, "resume")
                                                        app.market_running = false
                                                    elseif app.market_running == false then
                                                        market:resume()
                                                        bb:Update(bb.mP, bb.mD, app.nf, "pause")
                                                        app.market_running = true
                                                    end
                                                end,
                                                en = "run_stop_button",
                                            },
                                        }, CR { "pan", 0.1 }
                                    ),
                                }, CR(6))
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
                    V(
                        {
                            ui_table_padding,
                            E,
                            H(
                                {
                                    B { t = "Interest Rate:" },
                                    B { t = "Interest Rate: ", en = "market_interest_logger" },
                                }, CR(2)),
                            B { t = "This is a logger", en = "market_logger" },
                        }, CR { 0.7, "pan", 0.05, 0.05 })
                }
                --============================
            }, { 0.7, 0.001, 0.3 })


        --== POST UI STUFF
        ui_layout:Update(vec3(0, 0, app.bd), vec3(app.fd.x, app.fd.y, 1))
        ui_graph.Config({ from = vec3(0), to = vec3(20) }).Scale().Points {}


        local market_logger_elemn = app.els["market_logger"]
        local market_interest_logger_elemn = app.els["market_interest_logger"]
        app.wr.c:Push(Jkr.CreateUpdatable(
            function()
                market_logger_elemn:Update(market_logger_elemn.mP,
                    market_logger_elemn.mD,
                    app.nf,
                    hft.GetLogString(),
                    nil, hft.GetLogColor())
                market_interest_logger_elemn:Update(market_interest_logger_elemn.mP,
                    market_interest_logger_elemn.mD,
                    app.nf,
                    "" .. market:get_interest_rate(),
                    nil, hft.GetLogColor())

                local data = {
                    { "%d", "%s", "%.2f" },
                }
                local stocks = market:get_stocks()
                for i = 1, #stocks do
                    data[#data + 1] = { i, stocks[i]:get_name(), stocks[i]:get_price() }
                end
                ui_table:Update(ui_table.mP, ui_table.mD, data, 0)
            end
        ))


        do
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
end
