#pragma once
#include <ksai_config.hpp>

namespace hft {

using namespace ksai;
template <typename T> using urdis = std::uniform_real_distribution<T>;
template <typename T> using uidis = std::uniform_int_distribution<T>;

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

      stock(s in_name, double in_price);
      double get_price() const;
      void fluctuate(span<double> in_fluctuations);
      void edit_price(double in_amount);
      void economic_indicators(double in_factor, double in_impact);
};

struct order {
      enum class type { buy, sell };
      enum class status { open, partial, closed, cancelled };
      enum class preference { limit, market };

      double m_price;
      int m_id;
      int m_quantity;

      order::type m_type;
      sp<stock> m_stock;
      order::status m_status = order::status::open;
      order::preference m_pref;
      order(order::type in_ordertype,
            sp<stock> in_stock,
            int in_quantity,
            int in_id,
            order::preference in_pref);
};

inline bool operator==(sp<order> in_orderA, sp<order> in_orderB) {
      if (in_orderA->m_id == in_orderB->m_id and in_orderA->m_stock == in_orderB->m_stock and
          in_orderA->m_quantity == in_orderB->m_quantity) {
            return true;
      } else {
            return false;
      }
}

struct trade {
      const sp<order> m_buy_order;
      const sp<order> m_sell_order;
      int m_quantity = 0;
      trade(sp<order> in_buy_order, sp<order> in_sell_order);
};

struct orderBook {
      ///@note Greater than produces min heap
      ///@note Less than produces max heap
      struct greater {
            bool operator()(sp<order> a, sp<order> b) { return a->m_price > b->m_price; }
      };
      struct less {
            bool operator()(sp<order> a, sp<order> b) { return a->m_price < b->m_price; }
      };

      using min_heap = std::priority_queue<sp<order>, v<sp<order>>, greater>;
      using max_heap = std::priority_queue<sp<order>, v<sp<order>>, less>;
      map<s, max_heap> m_buy_orders;
      map<s, min_heap> m_sell_orders;

      orderBook();
      void add_order(sp<order> in_order, mtx &in_trademtx);
      v<sp<order>> execute_trades(mtx &in_ordmtx);
      bool cancel_order(sp<order> in_order, mtx &in_trademtx);
      void clear();
};

struct portfolio {
      map<sp<stock>, int> m_holdings; // Stock holdings
      portfolio();
      double make_change(sp<order> in_order, double in_available_cash);
      void cancel_sell(sp<order> in_order);
      void register_stock_bought(sp<stock> in_stock, int in_quantity);
      double register_stock_sold(sp<stock> in_stock, int in_quantity, double in_cash);

      v<sp<stock>> list_stocks();
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

      trader(int in_id, double in_cash, sp<orderBook> in_orderBook);

      void make_order(order::type in_type,
                      sp<stock> in_stock,
                      int in_quantity,
                      order::preference in_pref,
                      mtx &in_trademtx);
      ///@note mutex needed ?
      void update_portfolio(sp<order> in_order);
      void cancel_order(sp<order> in_order, mtx &in_trademtx);
      void do_random_action(v<sp<stock>> in_stocks, mtx &in_trademtx);
};

struct market {
      ///@note somethings

      double m_interest_rate = 1.0;
      double factors         = 1.0;

      GETTER get_interest_rate() { return m_interest_rate; }

      ///@note random Devices
      randev m_rand;
      rangen m_rang; /// std::mt19937 generator

      ///@note stuffs
      v<sp<trader>> m_traders;
      v<sp<stock>> m_stocks;
      sp<orderBook> m_orderBook = mksh<orderBook>();

      GETTER get_stocks() { return m_stocks; }
      GETTER get_traders() { return m_traders; }

      ///@note mutexes
      mtx m_fluctuation_mtx;
      mtx m_order_mtx;
      mtx m_trade_mtx;
      mtx m_event_mtx;

      market();
      ~market();
      void add_trader(int in_id, double in_cash);
      void add_stock(s in_name, double in_price);

      std::thread m_event_thread;
      std::thread m_traders_thread;
      atm<bool> m_running;
      atm<bool> m_paused;
      void run();
      void reset();

      void stop();
      void pause();
      void resume();

      void fluctuate(mtx &in_fluc_mtx);
      event generate_random_market_event();
      void execute_order_book(mtx &in_ordmtx);
      void apply_market_impact(event in_event, mtx &in_event_mtx);
};

} // namespace hft
