#pragma once
#include <ksai_config.hpp>

namespace hft
{

    using namespace ksai;
    template <typename T>
    using urdis = std::uniform_real_distribution<T>;
    template <typename T>
    using uidis = std::uniform_int_distribution<T>;

    enum class event
    {
        change_in_interest_rate,
        inflation_rise,
        inflation_fall,
        deflation,
        recession,
        prosperity,
        change_in_global_economy,
        reports_of_economic_indicators,
        employment_data_release,
        gdp_announcement,
        trade_balance_report,
        central_bank_meeting,
        government_policy_change,
        new_tax_laws,
        fiscal_stimulus,
        sanctions_announced,

        earnings_report,
        dividend_announcement,
        stock_split,
        acquisition_merger_news,
        insider_trading_news,
        large_fund_liquidation,
        short_squeeze,
        ipo_announcement,

        war_breakout,
        peace_agreement,
        diplomatic_tensions,
        election_result,
        political_instability,
        cyberattack_on_infrastructure,

        natural_disaster,
        pandemic_news,
        energy_crisis,
        oil_price_spike,
        commodity_price_crash,

        opinion_of_the_public,
        viral_news_on_social_media,
        hedge_fund_opinion,
        market_sentiment_bullish,
        market_sentiment_bearish,
        sudden_volume_surge,
        unexpected_volatility,

        crossing_moving_average,
        price_breaks_support,
        price_breaks_resistance,
        flash_crash_detected,

        nothing
    };

    struct stock
    {
        atm<double> m_price;
        atm<double> m_factors;

        s m_name;

        ///@note random Devices
        randev m_rand;
        rangen m_rang; /// std::mt19937 generator

        stock(s &in_name, double in_price) : m_name(in_name), m_price(in_price) { m_factors = 1.0; }
        double get_price() const { return m_factors * m_price; }
        void flutuate(span<double> in_fluctuations)
        {
            urdis<double> distribution(0, 1.0);
            double fluctuation = distribution(m_rang);
            double intensity = distribution(m_rang);
            bool pos = true;
            if (fluctuation > (in_fluctuations[0] / 3))
            {
                pos = in_fluctuations[1] <= 0.5;
                double price_change = (intensity > in_fluctuations[2]) ? intensity / 12 : intensity / 18;
                edit_price(pos ? -price_change : price_change);
            }
        }
        void edit_price(double in_amount) { m_price = m_price * (1 + in_amount); }
        void economic_indicators(double in_factor, double in_impact)
        {
            urdis<double> distribution(0.0, in_factor * 2);
            double economic_report = distribution(m_rang);
            double stock_effect = 0.5;
            if (economic_report > stock_effect)
            {
                m_factors = in_factor * (in_impact + economic_report);
            }
        }
    };

    struct order
    {
        enum class type
        {
            buy,
            sell
        };
        enum class status
        {
            open,
            partial,
            closed,
            cancelled
        };
        enum class preference
        {
            limit,
            market
        };

        double m_price;
        int m_id;
        int m_quantity;

        order::type m_type;
        sp<stock> m_stock;
        order::status m_status = order::status::open;
        order::preference m_pref;
        order(order::type in_ordertype, sp<stock> in_stock, int in_quantity, int in_id, order::preference in_pref)
        {
            m_type = in_ordertype;
            m_stock = in_stock;
            m_quantity = in_quantity;
            m_id = in_id;
            m_pref = in_pref;
        }
    };

    inline bool operator==(sp<order> in_orderA, sp<order> in_orderB)
    {
        if (in_orderA->m_id == in_orderB->m_id and
            in_orderA->m_stock == in_orderB->m_stock and
            in_orderA->m_quantity == in_orderB->m_quantity)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    struct trade
    {
        const sp<order> m_buy_order;
        const sp<order> m_sell_order;
        int m_quantity = 0;
        trade(sp<order> in_buy_order, sp<order> in_sell_order)
            : m_buy_order(in_buy_order),
              m_sell_order(in_sell_order)
        {
            m_quantity = std::min(in_buy_order->m_quantity, in_sell_order->m_quantity);
        }
    };

    struct orderBook
    {
        ///@note Greater than produces min heap
        ///@note Less than produces max heap
        struct greater
        {
            bool operator()(sp<order> a, sp<order> b) { return a->m_price > b->m_price; }
        };
        struct less
        {
            bool operator()(sp<order> a, sp<order> b) { return a->m_price < b->m_price; }
        };

        using min_heap = std::priority_queue<sp<order>, v<sp<order>>, greater>;
        using max_heap = std::priority_queue<sp<order>, v<sp<order>>, less>;
        map<s, max_heap> m_buy_orders;
        map<s, min_heap> m_sell_orders;

        orderBook() {}
        void add_order(sp<order> in_order, mtx &in_trademtx)
        {
            std::lock_guard<mtx> lock(in_trademtx);
            if (in_order->m_type == order::type::buy)
            {
                m_buy_orders[in_order->m_stock->m_name].push(in_order);
            }
            else
            {
                m_sell_orders[in_order->m_stock->m_name].push(in_order);
            }
        }
        v<sp<order>> execute_trades(mtx &in_ordmtx)
        {
            std::lock_guard<mtx> lock(in_ordmtx);
            v<sp<order>> orders;
            for (auto &[stock_name, buy_queue] : m_buy_orders)
            {
                auto &sell_queue = m_sell_orders[stock_name];
                while (not buy_queue.empty() and not sell_queue.empty())
                {
                    auto buy_order = buy_queue.top();
                    auto sell_order = sell_queue.top();
                    bool buyer_is_offering_enough = buy_order->m_price >= sell_order->m_price;
                    bool market_order_fast = buy_order->m_pref == order::preference::market or sell_order->m_pref == order::preference::market;

                    if (buyer_is_offering_enough or market_order_fast)
                    {
                        trade trade(buy_order, sell_order);
                        buy_order->m_quantity -= trade.m_quantity;
                        sell_order->m_quantity -= trade.m_quantity;
                        if (buy_order->m_quantity == 0)
                        {
                            buy_queue.pop();
                            buy_order->m_status = order::status::closed;
                        }
                        else
                        {
                            buy_order->m_status = order::status::partial;
                        }
                        if (sell_order->m_quantity == 0)
                        {
                            sell_queue.pop();
                            sell_order->m_status = order::status::closed;
                        }
                        else
                        {
                            sell_order->m_status = order::status::partial;
                        }
                        orders.push_back(buy_order);
                        orders.push_back(sell_order);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            return orders;
        }
        bool cancel_order(sp<order> in_order, mtx &in_trademtx)
        {
            std::lock_guard<mtx> lock(in_trademtx);
            bool had_entry = false;
            if (in_order->m_type == order::type::buy)
            {
                auto &main_queue = m_buy_orders[in_order->m_stock->m_name];
                rmrf<decltype(main_queue)> temp_queue = main_queue;
                while (not temp_queue.empty())
                {
                    auto current_order = main_queue.top();
                    main_queue.pop();
                    if (in_order == current_order and had_entry)
                    {
                        had_entry = true;
                    }
                    else
                    {
                        temp_queue.push(current_order);
                    }
                }
                main_queue = temp_queue;
            }
            else
            {
                auto &main_queue = m_sell_orders[in_order->m_stock->m_name];
                rmrf<decltype(main_queue)> temp_queue = main_queue;
                while (not temp_queue.empty())
                {
                    auto current_order = main_queue.top();
                    main_queue.pop();
                    if (in_order == current_order and had_entry)
                    {
                        had_entry = true;
                    }
                    else
                    {
                        temp_queue.push(current_order);
                    }
                }
                main_queue = temp_queue;
            }
            return had_entry;
        }
        void clear()
        {
            m_buy_orders.clear();
            m_sell_orders.clear();
        }
    };

    struct portfolio
    {
        map<sp<stock>, int> m_holdings; // Stock holdings
        portfolio() {}
        double make_change(sp<order> in_order, double in_available_cash)
        {
            if (in_order->m_type == order::type::buy)
            {
                m_holdings[in_order->m_stock] += in_order->m_quantity;
            }
            else
            {
                m_holdings[in_order->m_stock] -= in_order->m_quantity;
                in_available_cash += in_order->m_price * in_order->m_quantity;
                if (m_holdings[in_order->m_stock] == 0)
                {
                    m_holdings.erase(in_order->m_stock);
                }
            }
            return in_available_cash;
        }
        void cancel_sell(sp<order> in_order)
        {
            m_holdings[in_order->m_stock] += in_order->m_quantity;
        }
        void register_stock_bought(sp<stock> in_stock, int in_quantity)
        {
            m_holdings[in_stock] += in_quantity;
        }
        double register_stock_sold(sp<stock> in_stock, int in_quantity, double in_cash)
        {
            m_holdings[in_stock] -= in_quantity;
            in_cash += in_stock->get_price();
            if (m_holdings[in_stock] == 0)
            {
                m_holdings.erase(in_stock);
            }
            return in_cash;
        }

        v<sp<stock>> list_stocks()
        {
            v<sp<stock>> out;
            for (const auto &pair : m_holdings)
            {
                out.push_back(pair.first);
            }
            return out;
        }
    };

    struct trader
    {
        portfolio m_porfolio;
        sp<orderBook> m_orderBook;
        v<sp<order>> m_activeOrders;

        int m_id;
        double m_available_cash;

        ///@note random Devices
        randev m_rand;
        rangen m_rang; /// std::mt19937 generator

        trader(int in_id, double in_cash, sp<orderBook> in_orderBook) : m_id(in_id),
                                                                        m_available_cash(in_cash),
                                                                        m_orderBook(in_orderBook)
        {
        }

        void make_order(order::type in_type,
                        sp<stock> in_stock,
                        int in_quantity,
                        order::preference in_pref,
                        mtx &in_trademtx)
        {
            sp<order> order_ = mksh<order>(in_type, in_stock, in_quantity, m_id, in_pref);
            double order_price = order_->m_price;
            if (order_->m_type == order::type::buy)
            {
                {
                    std::lock_guard<mtx> lock(in_trademtx);
                    double moneychange = order_price * in_quantity;
                    m_available_cash -= moneychange;
                    m_activeOrders.push_back(order_);
                }
                m_orderBook->add_order(order_, in_trademtx);
            }
            else
            {
                {
                    std::lock_guard<mtx> lock(in_trademtx);
                    m_porfolio.m_holdings[in_stock] -= in_quantity;
                    if (m_porfolio.m_holdings[in_stock] == 0)
                    {
                        m_porfolio.m_holdings.erase(in_stock);
                    }
                }
                m_orderBook->add_order(order_, in_trademtx);
            }
        }
        ///@note mutex needed ?
        void update_portfolio(sp<order> in_order)
        {
            m_available_cash = m_porfolio.make_change(in_order, m_available_cash);
        }
        void cancel_order(sp<order> in_order, mtx &in_trademtx)
        {
            bool cancel = m_orderBook->cancel_order(in_order, in_trademtx);
            {
                std::lock_guard<mtx> locak(in_trademtx);
                if (cancel and in_order->m_type == order::type::buy)
                {
                    m_available_cash += in_order->m_quantity * in_order->m_price;
                }
                else
                {
                    m_porfolio.cancel_sell(in_order);
                }
            }
        }
        void do_random_action(v<sp<stock>> in_stocks, mtx &in_trademtx)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2s);

            urdis<double> normal_distribution(0, 1.0);
            uidis<int> buy_distribution(0, in_stocks.size() - 1);
            double random_action = normal_distribution(m_rang);
            double random_action_type = normal_distribution(m_rang);

            auto holding_count = m_porfolio.m_holdings.size();
            bool has_holdings = holding_count > 0;

            ///@note Sell on these
            ///
            ///
            ///
            ///
            if (has_holdings and random_action < 0.22)
            {
                uidis<int> sell_distribution(0, holding_count - 1);
                int choice = sell_distribution(m_rang);
                auto stock = m_porfolio.list_stocks()[choice];

                uidis<int> sell_quantity_distribution(1, m_porfolio.m_holdings[stock]);
                order::preference pref = order::preference::limit;
                int sell_quantity = sell_quantity_distribution(m_rang);

                if (random_action_type > 0.5)
                {
                    pref = order::preference::market;
                }
                make_order(order::type::sell, stock, sell_quantity, pref, in_trademtx);
            }
            else if (has_holdings and random_action < 0.44)
            {
                uidis<int> sell_distribution(0, holding_count - 1);
                int choice = sell_distribution(m_rang);
                auto stock = m_porfolio.list_stocks()[choice];

                uidis<int> sell_quantity_distribution(1, m_porfolio.m_holdings[stock]);
                int sell_quantity = sell_quantity_distribution(m_rang);

                m_available_cash = m_porfolio.register_stock_sold(stock, sell_quantity, m_available_cash);
            }
            ///@note Buy on these
            ///
            ///
            ///
            ///
            else if (random_action < 0.70)
            {
                uidis<int> buy_distribution(0, holding_count - 1);
                int choice = buy_distribution(m_rang);
                auto stock = in_stocks[choice];

                uidis<int> buy_quantity_distribution(1, (int)std::floor(m_available_cash / stock->get_price()));
                int buy_quantity = buy_quantity_distribution(m_rang);

                m_available_cash -= (stock->get_price() * buy_quantity);
                m_porfolio.register_stock_bought(stock, buy_quantity);
            }
            else if (random_action < 0.96)
            {
                uidis<int> buy_distribution(0, holding_count - 1);
                int choice = buy_distribution(m_rang);
                auto stock = in_stocks[choice];

                uidis<int> buy_quantity_distribution(1, (int)std::floor(m_available_cash / stock->get_price()));
                int buy_quantity = buy_quantity_distribution(m_rang);

                order::preference pref = order::preference::limit;
                if (random_action_type > 0.5)
                {
                    pref = order::preference::market;
                }
                make_order(order::type::buy, stock, buy_quantity, pref, in_trademtx);
            }
            else
            {
                uidis<int> order_cancel_distribution(0, m_activeOrders.size() - 1);
                int choice = buy_distribution(m_rang);
                if (!(m_activeOrders.size() == 0))
                {
                    cancel_order(m_activeOrders[choice], in_trademtx);
                }
            }
        }
    };

    struct market
    {
        ///@note somethings
        double m_interest_rate = 1.0;
        double factors = 1.0;

        ///@note random Devices
        randev m_rand;
        rangen m_rang; /// std::mt19937 generator

        ///@note stuffs
        v<sp<trader>> m_traders;
        v<sp<stock>> m_stocks;
        sp<orderBook> m_orderBook = mksh<orderBook>();

        ///@note mutexes
        mtx m_fluctuation_mtx;
        mtx m_order_mtx;
        mtx m_trade_mtx;
        mtx m_event_mtx;

        market() {}
        void add_trader(int in_id, double in_cash)
        {
            m_traders.push_back(mksh<trader>(in_id, in_cash, m_orderBook));
        }
        void add_stock(s in_name, double in_price)
        {
            m_stocks.push_back(mksh<stock>(in_name, in_price));
        }
        void run() {}
        void reset()
        {
            m_traders.clear();
            m_stocks.clear();
            m_orderBook.reset();
        }
        void fluctuate() {}

        event generateRandomMarketEvent()
        {
            using e = event;
            static const std::vector<e> all_events = {
                e::change_in_interest_rate,        // 5
                e::inflation_rise,                 // 5
                e::inflation_fall,                 // 5
                e::deflation,                      // 3
                e::recession,                      // 3
                e::prosperity,                     // 3
                e::change_in_global_economy,       // 5
                e::reports_of_economic_indicators, // 10
                e::employment_data_release,        // 10
                e::gdp_announcement,               // 5
                e::trade_balance_report,           // 5
                e::central_bank_meeting,           // 5
                e::government_policy_change,       // 3
                e::new_tax_laws,                   // 2
                e::fiscal_stimulus,                // 2
                e::sanctions_announced,            // 1

                e::earnings_report,         // 5
                e::dividend_announcement,   // 3
                e::stock_split,             // 2
                e::acquisition_merger_news, // 3
                e::insider_trading_news,    // 2
                e::large_fund_liquidation,  // 1
                e::short_squeeze,           // 2
                e::ipo_announcement,        // 3

                e::war_breakout,                  // 1
                e::peace_agreement,               // 2
                e::diplomatic_tensions,           // 2
                e::election_result,               // 3
                e::political_instability,         // 2
                e::cyberattack_on_infrastructure, // 1

                e::natural_disaster,      // 1
                e::pandemic_news,         // 1
                e::energy_crisis,         // 1
                e::oil_price_spike,       // 2
                e::commodity_price_crash, // 2

                e::opinion_of_the_public,      // 10
                e::viral_news_on_social_media, // 10
                e::hedge_fund_opinion,         // 5
                e::market_sentiment_bullish,   // 10
                e::market_sentiment_bearish,   // 10
                e::sudden_volume_surge,        // 5
                e::unexpected_volatility,      // 5

                e::crossing_moving_average, // 10
                e::price_breaks_support,    // 10
                e::price_breaks_resistance, // 10
                e::flash_crash_detected,    // 1

                e::nothing // 20 (high chance nothing happens)
            };

            static const std::vector<int> weights = {
                5, 5, 5, 3, 3, 3, 5, 10, 10, 5, 5, 5, 3, 2, 2, 1,
                5, 3, 2, 3, 2, 1, 2, 3,
                1, 2, 2, 3, 2, 1,
                1, 1, 1, 2, 2,
                10, 10, 5, 10, 10, 5, 5,
                10, 10, 10, 1,
                20};

            std::discrete_distribution<int> dist(weights.begin(), weights.end());
            return all_events[dist(m_rang)];
        }

        void execute_order_book(mtx &in_ordmtx)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(4s);
            v<sp<order>> orders = m_orderBook->execute_trades(in_ordmtx);
            if (not orders.empty())
            {
                for (auto order : orders)
                {
                    for (auto trader : m_traders)
                    {
                        if (trader->m_id == order->m_id)
                        {
                            trader->update_portfolio(order);
                        }
                    }
                }
            }
        }
        void apply_market_impact(event in_event, mtx &in_event_mtx)
        {
            v<double> fluctuations;
            auto applyFactorToAll = [&](double factor)
            {
                for (auto &stock : m_stocks)
                    stock->m_factors = factor;
            };

            auto applyFluctuationToAll = [&](std::span<double> flucs)
            {
                for (auto &stock : m_stocks)
                    stock->flutuate(flucs);
            };

            std::uniform_real_distribution<double> small(-0.05, 0.05);
            std::uniform_real_distribution<double> moderate(-0.15, 0.15);
            std::uniform_real_distribution<double> strong(-0.3, 0.3);
            double impact;

            switch (in_event)
            {
            case event::change_in_interest_rate:
                impact = std::uniform_real_distribution<double>(-4.0, 4.0)(m_rang);
                m_interest_rate = std::clamp(m_interest_rate + impact, 0.01, 20.0);
                applyFactorToAll(factors * (1.0 + (impact / 100.0)));
                break;

            case event::inflation_rise:
                applyFactorToAll(factors * 0.95);
                break;

            case event::inflation_fall:
                applyFactorToAll(factors * 1.05);
                break;

            case event::deflation:
                applyFactorToAll(factors * 0.9);
                break;

            case event::recession:
                applyFactorToAll(factors * 0.7);
                break;

            case event::prosperity:
                applyFactorToAll(factors * 1.3);
                break;

            case event::change_in_global_economy:
            case event::central_bank_meeting:
            case event::government_policy_change:
            case event::new_tax_laws:
                impact = moderate(m_rang);
                applyFactorToAll(factors * (1.0 + impact));
                break;

            case event::reports_of_economic_indicators:
            case event::employment_data_release:
            case event::gdp_announcement:
            case event::trade_balance_report:
                impact = std::uniform_real_distribution<double>(0.25, 0.5)(m_rang);
                for (auto &stock : m_stocks)
                    stock->economic_indicators(factors, impact);
                break;

            case event::fiscal_stimulus:
            case event::sanctions_announced:
                applyFactorToAll(factors * (in_event == event::fiscal_stimulus ? 1.2 : 0.8));
                break;

            case event::earnings_report:
            case event::dividend_announcement:
            case event::stock_split:
            case event::ipo_announcement:
                applyFactorToAll(factors * 1.05);
                break;

            case event::acquisition_merger_news:
                applyFactorToAll(factors * 1.1);
                break;

            case event::insider_trading_news:
            case event::large_fund_liquidation:
            case event::short_squeeze:
                applyFactorToAll(factors * (1.0 + strong(m_rang)));
                break;

            case event::war_breakout:
            case event::political_instability:
            case event::cyberattack_on_infrastructure:
            case event::natural_disaster:
            case event::pandemic_news:
            case event::energy_crisis:
            case event::oil_price_spike:
                applyFactorToAll(factors * 0.75);
                break;

            case event::peace_agreement:
                applyFactorToAll(factors * 1.1);
                break;

            case event::diplomatic_tensions:
            case event::election_result:
                applyFactorToAll(factors * (1.0 + moderate(m_rang)));
                break;

            case event::commodity_price_crash:
                applyFactorToAll(factors * 0.85);
                break;

            case event::opinion_of_the_public:
            case event::viral_news_on_social_media:
            case event::hedge_fund_opinion:
            {
                std::vector<double> flucs = {
                    std::uniform_real_distribution<double>(0.1, 0.6)(m_rang),
                    std::uniform_real_distribution<double>(-0.5, 0.5)(m_rang),
                    std::uniform_real_distribution<double>(0.1, 0.4)(m_rang)};
                applyFluctuationToAll(flucs);
                break;
            }

            case event::market_sentiment_bullish:
                applyFactorToAll(factors * 1.15);
                break;

            case event::market_sentiment_bearish:
                applyFactorToAll(factors * 0.85);
                break;

            case event::sudden_volume_surge:
            case event::unexpected_volatility:
                applyFactorToAll(factors * (1.0 + small(m_rang)));
                break;

            case event::crossing_moving_average:
            case event::price_breaks_support:
            case event::price_breaks_resistance:
            case event::flash_crash_detected:
                applyFactorToAll(factors * 0.9);
                break;

            case event::nothing:
                std::cout << "Nothing significant happened." << std::endl;
                break;
            }
        }
    };

}
