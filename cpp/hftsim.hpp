#pragma once
#include <ksai_config.hpp>

namespace hft {

using namespace ksai;

enum class event {
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


struct stock {
    atm<double> m_price;
    atm<double> m_factors;

    s m_name;

    ///@note random Devices
    randev m_rand;
    rangen m_rang; /// std::mt19937 generator

    stock(s& in_name, double in_price) : m_name(in_name), m_price(in_price) { m_factors = 1.0; }
    double get_price() const { return m_factors * m_price; }
    void flutuate(span<double> in_fluctuations) {
        std::uniform_real_distribution<double> distribution(0, 1.0);
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
    void economic_indicators(double in_factor, double in_impact) {
        std::uniform_real_distribution<double> distribution(0.0, in_factor * 2);
        double economic_report = distribution(m_rang);
        double stock_effect = 0.5;
        if (economic_report > stock_effect)
        {
            m_factors = in_factor * (in_impact + economic_report);
        }
    }
};


struct order {
    enum class type {buy, sell};
    enum class status {open, partial, closed, cancelled};
    enum class preference {limit, market};

    double m_price;
    int id;
    int m_quantity;

    order::type m_type;
    sp<stock> m_stock;
    order::status m_status = order::status::open;
    order::preference m_pref;
    order(order::type in_ordertype, sp<stock> in_stock, int in_quantity, order::preference in_pref)
    {
        m_type = in_ordertype;
        m_stock = in_stock;
        m_quantity = in_quantity;
        m_pref = in_pref;
    }
};

struct orderBook {

    ///@note Greater than produces min heap
    ///@note Less than produces max heap
    struct compare_buy_order {
        bool operator()(sp<order> a, sp<order> b) { return a->m_price > b->m_price; }    
    };
    struct compare_sell_order {
        bool operator()(sp<order> a, sp<order> b) { return a->m_price < b->m_price; }    
    };

    using min_heap = std::priority_queue<sp<order>, v<sp<order>>, compare_buy_order>;
    using max_heap = std::priority_queue<sp<order>, v<sp<order>>, compare_sell_order>;
    map<s, min_heap> m_buy_orders;
    map<s, max_heap> m_sell_orders;

    orderBook() {}
    void add_order(sp<order> in_order, mtx& in_trademtx) {
        std::lock_guard<mtx> lock(in_trademtx);
        if (in_order->m_type == order::type::buy) {
            m_buy_orders[in_order->m_stock->m_name].push(in_order);
        }
        else {
            m_sell_orders[in_order->m_stock->m_name].push(in_order);
        }
    }
    v<sp<order>> execute_trades(mtx& in_ordmtx) {}
    bool cancel_order(sp<order> in_order, mtx& in_trademtx) {}
    void clear() {
        m_buy_orders.clear();
        m_sell_orders.clear();
    }
};

struct portfolio {
    map<sp<stock>, int> m_holdings;
    portfolio() {}
    double make_change(sp<order> in_order, double in_cash) {}
    v<sp<stock>> list_stocks() {}
    void cancel_sell(sp<order> in_order) {}
    void buy_stock(sp<stock> in_stock, int in_quantity) {}
    double sell_stock(sp<stock> in_stock, int in_quantity, double in_cash) {}
};

struct trade {
    const sp<order> m_buy_order;
    const sp<order> m_sell_order;
    int m_trade_quantity = 0;
    trade(sp<order> in_buy_order, sp<order> in_sell_order) 
        : m_buy_order(in_buy_order),
        m_sell_order(in_sell_order)
	{
        if (in_buy_order->m_quantity < in_sell_order->m_quantity)
        {
            m_trade_quantity = m_buy_order->m_quantity;
        }
        else {
            m_trade_quantity = m_sell_order->m_quantity;
        }
    }
};

struct trader {
    portfolio m_porfolio;
    sp<orderBook> m_orderBook;
    v<sp<order>> m_activeOrders;

    int m_id;
    double m_available_cash;

    ///@note random Devices
    randev m_rand;
    rangen m_rang; /// std::mt19937 generator

    void make_order(order::type in_type,
         sp<stock> in_stock,
         int in_quantity,
         order::preference in_pref,
         mtx& in_trademtx) {}
    void update_portfolio(sp<order> in_order, mtx& in_ordmtx) {}
    void cancel_order(sp<order> in_order, mtx& in_trademtx) {}
    void do_action(v<sp<stock>> in_stocks, mtx& in_trademtx) {}
};

struct market {
          ///@note somethings
          double m_interest_rate = 1.0;
          double factors = 1.0;

          ///@note random Devices
          randev m_rand;
          rangen m_rang; /// std::mt19937 generator

          ///@note stuffs
          v<sp<trader>> m_traders;

          ///@note mutexes
          mtx m_fluctuation_mtx;
          mtx m_order_mtx;
          mtx m_trade_mtx;
          mtx m_event_mtx;

          market() {}
          void init_traders() {}
          void init_stocks() {}

          void run() {}
          void reset() {}
          void fluctuate() {}

          void generate_market_event() {}
          void execute_order_book() {}
          void apply_market_impact() {}
};

}
