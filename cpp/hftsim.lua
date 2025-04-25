require "bundle"
function main()
    require "hftsim"

    local lerp = Jmath.GetLerps()
    local app = Engine.GameFramework({
        validation = true,
        bc = vec4(0, 0, 0, 1),
        wd = vec2(1500, 800),
        fd = vec2(1500, 800),
        heading = "ksai HFT"
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
            tb = vec4(0, 0.9, 0.9, 1),
            bullish = vec4(0.0, 1, 0.0, 1),
            bearish = vec4(1, 0.0, 0.0, 1),
            stable = vec4(0.0, 0.0, 0.0, 1),
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


        local ui_market_window   = Window("Market", c.t, vec4(0.5, 0.5, 0.5, 1))
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
                        S {
                            ui_market_window,
                            H({ E, app.Pad(
                                V(
                                    {
                                        B { t = "Bullish", bc = c.bullish },
                                        B { t = "Bearish", bc = c.bearish },
                                        B { t = "Between", bc = c.stable, c = vec4(1) },
                                        E,
                                    }, CR { 0.1, 0.1, 0.1, "pan" })
                            ) }, CR { "pan", 0.1 })
                        },


                        --============================
                        E,

                        --@note Control Window
                        --============================
                        S { ui_controls_window,
                            V(
                                {
                                    E,
                                    H(
                                        {
                                            B { t = "Graph of", bc = vec4(0, 0.9, 0, 1) },
                                            app.Sel {
                                                en = "market_current_symbol",
                                                fl = function()
                                                    return "NABIL"
                                                end,
                                                fr = function()
                                                    return "HBCL"
                                                end,
                                                display = function()
                                                    app.market_current_symbol = "NABIL"
                                                    return app.market_current_symbol
                                                end
                                            },
                                            E
                                        }, CR { 0.2, 0.3, "pan" }
                                    ),
                                    H(
                                        {
                                            B { t = "Delay (ms)", bc = vec4(0, 0.9, 0, 1) },
                                            app.Sel {
                                                en = "market_execution_delay",
                                                fl = function()
                                                    if app.market_execution_delay > 0.005 then
                                                        app.market_execution_delay = app.market_execution_delay - 0.005
                                                    end
                                                    return string.format("%.4f", app.market_execution_delay)
                                                end,
                                                fr = function()
                                                    app.market_execution_delay = app.market_execution_delay + 0.005
                                                    return string.format("%.4f", app.market_execution_delay)
                                                end,
                                                display = function()
                                                    if not app.market_execution_delay then
                                                        app.market_execution_delay = 0.01
                                                    end
                                                    return string.format("%.4f", app.market_execution_delay)
                                                end
                                            },
                                            E,
                                        },
                                        CR { 0.2, 0.3, "pan" }
                                    ),
                                    E,
                                    H(
                                        {
                                            B { t = "Graph Time", bc = vec4(0, 0.9, 0, 1) },
                                            B { t = "reset", onclick = function()
                                                app.market_graph_time = 0.01
                                            end },
                                            B { t = "Graph Canvas", bc = vec4(0, 0.9, 0, 1) },
                                            B { t = "clear", onclick = function()
                                                ui_graph.Clear()
                                            end },
                                            E
                                        }, CR { 0.2, 0.1, 0.2, 0.1, "pan" }
                                    ),
                                    H(
                                        {
                                            E,
                                            B {
                                                t = "reset",
                                                onclick = function()
                                                    market:reset()
                                                end
                                            },
                                            B {
                                                t = "stop",
                                                onclick = function()
                                                    market:stop()
                                                end
                                            },
                                            B {
                                                t = "run",
                                                onclick = function()
                                                    local bb = app.els["run_stop_button"]
                                                    if type(app.market_running) == "nil" then
                                                        market:run()
                                                        app.graph_running = true
                                                        app.market_running = true
                                                        bb:Update(bb.mP, bb.mD, app.nf, "pause")
                                                    elseif app.market_running == true then
                                                        market:pause()
                                                        app.graph_running = false
                                                        bb:Update(bb.mP, bb.mD, app.nf, "resume")
                                                        app.market_running = false
                                                    elseif app.market_running == false then
                                                        market:resume()
                                                        app.graph_running = true
                                                        bb:Update(bb.mP, bb.mD, app.nf, "pause")
                                                        app.market_running = true
                                                    end
                                                end,
                                                en = "run_stop_button",
                                            },
                                        }, CR { "pan", 0.1, 0.1, 0.1 }
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
                            app.Pad(
                                app.Sel {
                                    en = "market_sdv",
                                    fl = function()
                                        if not app.market_sdv then
                                            app.market_sdv = 0
                                        end
                                        app.market_sdv = app.market_sdv - 1
                                        return tostring(app.market_sdv)
                                    end,
                                    fr = function()
                                        if not app.market_sdv then
                                            app.market_sdv = 0
                                        end
                                        app.market_sdv = app.market_sdv + 1
                                        return tostring(app.market_sdv)
                                    end,
                                    display = "0"
                                }, 0.25, 0.1
                            ),
                            E,
                            H(
                                {
                                    B { t = "Interest Rate:" },
                                    B { t = "Interest Rate: ", en = "market_interest_logger" },
                                }, CR(2)),
                            B { t = "This is a logger", en = "market_logger" },
                        }, CR { 0.7, 0.05, "pan", 0.05, 0.05 })
                }
                --============================
            }, { 0.7, 0.001, 0.3 })


        --== POST UI STUFF
        ui_layout:Update(vec3(0, 0, app.bd), vec3(app.fd.x, app.fd.y, 1))
        ui_graph.Config({ from = vec3(0), to = vec3(20, 5, 1) }).Scale().Points {}


        local market_logger_elemn = app.els["market_logger"]
        local market_interest_logger_elemn = app.els["market_interest_logger"]

        local market_symbol_lag_value
        app.market_graph_time = 0.01
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
                    local name = stocks[i]:get_name()
                    local price = stocks[i]:get_price()
                    data[#data + 1] = { i, name, price }
                    if app.graph_running and name == app.market_current_symbol then
                        if not market_symbol_lag_value then
                            market_symbol_lag_value = price
                        end
                        local delta = (price - market_symbol_lag_value) / 100
                        local thickness = 30
                        local color = c.stable
                        if delta > 0 then
                            thickness = 15
                            color = c.bullish
                        elseif delta < 0 then
                            thickness = 15
                            color = c.bearish
                        end
                        ui_graph.Points {
                            mat4(
                                vec4(app.market_graph_time, 0, app.market_graph_time, price / 1000),
                                vec4(color),
                                vec4(thickness),
                                vec4(0)
                            ),
                        }
                        market_symbol_lag_value = price
                        app.market_graph_time = app.market_graph_time + 0.01
                    end
                end
                ui_table:Update(ui_table.mP, ui_table.mD, data, app.market_sdv or 0)
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

            app.loop(function()
                market:stop()
            end)
        end
    end
end
