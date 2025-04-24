#include "hftsim.hpp"
#include <sol/sol.hpp>
#include <glm/glm.hpp>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

static std::string current_log;
static std::mutex logging_mutex;
static glm::vec4 current_color = glm::vec4(0, 0, 0, 1);
void bind_hft(sol::state_view inlua) {
      using namespace hft;
      sol::table hft = inlua.create_named_table("hft");
      hft.new_enum<false>("event",
                          "change_in_interest_rate",
                          hft::event::change_in_interest_rate,
                          "inflation_rise",
                          hft::event::inflation_rise,
                          "inflation_fall",
                          hft::event::inflation_fall,
                          "deflation",
                          hft::event::deflation,
                          "recession",
                          hft::event::recession,
                          "prosperity",
                          hft::event::prosperity,
                          "change_in_global_economy",
                          hft::event::change_in_global_economy,
                          "reports_of_economic_indicators",
                          hft::event::reports_of_economic_indicators,
                          "employment_data_release",
                          hft::event::employment_data_release,
                          "gdp_announcement",
                          hft::event::gdp_announcement,
                          "trade_balance_report",
                          hft::event::trade_balance_report,
                          "central_bank_meeting",
                          hft::event::central_bank_meeting,
                          "government_policy_change",
                          hft::event::government_policy_change,
                          "new_tax_laws",
                          hft::event::new_tax_laws,
                          "fiscal_stimulus",
                          hft::event::fiscal_stimulus,
                          "sanctions_announced",
                          hft::event::sanctions_announced,

                          "earnings_report",
                          hft::event::earnings_report,
                          "dividend_announcement",
                          hft::event::dividend_announcement,
                          "stock_split",
                          hft::event::stock_split,
                          "acquisition_merger_news",
                          hft::event::acquisition_merger_news,
                          "insider_trading_news",
                          hft::event::insider_trading_news,
                          "large_fund_liquidation",
                          hft::event::large_fund_liquidation,
                          "short_squeeze",
                          hft::event::short_squeeze,
                          "ipo_announcement",
                          hft::event::ipo_announcement,

                          "war_breakout",
                          hft::event::war_breakout,
                          "peace_agreement",
                          hft::event::peace_agreement,
                          "diplomatic_tensions",
                          hft::event::diplomatic_tensions,
                          "election_result",
                          hft::event::election_result,
                          "political_instability",
                          hft::event::political_instability,
                          "cyberattack_on_infrastructure",
                          hft::event::cyberattack_on_infrastructure,

                          "natural_disaster",
                          hft::event::natural_disaster,
                          "pandemic_news",
                          hft::event::pandemic_news,
                          "energy_crisis",
                          hft::event::energy_crisis,
                          "oil_price_spike",
                          hft::event::oil_price_spike,
                          "commodity_price_crash",
                          hft::event::commodity_price_crash,

                          "opinion_of_the_public",
                          hft::event::opinion_of_the_public,
                          "viral_news_on_social_media",
                          hft::event::viral_news_on_social_media,
                          "hedge_fund_opinion",
                          hft::event::hedge_fund_opinion,
                          "market_sentiment_bullish",
                          hft::event::market_sentiment_bullish,
                          "market_sentiment_bearish",
                          hft::event::market_sentiment_bearish,
                          "sudden_volume_surge",
                          hft::event::sudden_volume_surge,
                          "unexpected_volatility",
                          hft::event::unexpected_volatility,

                          "crossing_moving_average",
                          hft::event::crossing_moving_average,
                          "price_breaks_support",
                          hft::event::price_breaks_support,
                          "price_breaks_resistance",
                          hft::event::price_breaks_resistance,
                          "flash_crash_detected",
                          hft::event::flash_crash_detected,

                          "nothing",
                          hft::event::nothing);

      hft.new_enum<false>(
           "order_type", "buy", hft::order::type::buy, "sell", hft::order::type::sell);

      hft.new_enum<false>("order_status",
                          "open",
                          hft::order::status::open,
                          "partial",
                          hft::order::status::partial,
                          "closed",
                          hft::order::status::closed,
                          "cancelled",
                          hft::order::status::cancelled);

      hft.new_enum<false>("order_preference",
                          "limit",
                          hft::order::preference::limit,
                          "market",
                          hft::order::preference::market);

      hft.new_usertype<stock>(
           "stock",
           sol::call_constructor,
           sol::factories([](s in_name, double in_price) -> std::shared_ptr<stock> {
                 return mksh<stock>(in_name, in_price);
           }),
           "get_price",
           &stock::get_price,
           "get_name",
           &stock::get_name,
           "fluctuate",
           &stock::fluctuate,
           "edit_price",
           &stock::edit_price,
           "economic_indicators",
           &stock::economic_indicators);

      hft.new_usertype<order>("order",
                              sol::call_constructor,
                              sol::factories([](order::type in_type,
                                                sp<stock> in_stock,
                                                int in_quantity,
                                                int in_id,
                                                order::preference in_pref) {
                                    return mksh<order>(
                                         in_type, in_stock, in_quantity, in_id, in_pref);
                              }),
                              "m_price",
                              &order::m_price,
                              "m_id",
                              &order::m_id,
                              "m_quantity",
                              &order::m_quantity,
                              "m_type",
                              &order::m_type,
                              "m_stock",
                              &order::m_stock,
                              "m_status",
                              &order::m_status,
                              "m_pref",
                              &order::m_pref);

      hft.new_usertype<trade>("trade",
                              sol::call_constructor,
                              sol::factories([](sp<order> buy, sp<order> sell) {
                                    return std::make_shared<trade>(buy, sell);
                              }),
                              "m_buy_order",
                              &trade::m_buy_order,
                              "m_sell_order",
                              &trade::m_sell_order,
                              "m_quantity",
                              &trade::m_quantity);

      hft.new_usertype<orderBook>("orderBook",
                                  sol::call_constructor,
                                  sol::factories([]() { return std::make_shared<orderBook>(); }),
                                  "add_order",
                                  &orderBook::add_order,
                                  "execute_trades",
                                  &orderBook::execute_trades,
                                  "cancel_order",
                                  &orderBook::cancel_order,
                                  "clear",
                                  &orderBook::clear);

      hft.new_usertype<portfolio>("portfolio",
                                  sol::call_constructor,
                                  sol::factories([]() { return std::make_shared<portfolio>(); }),
                                  "make_change",
                                  &portfolio::make_change,
                                  "cancel_sell",
                                  &portfolio::cancel_sell,
                                  "register_stock_bought",
                                  &portfolio::register_stock_bought,
                                  "register_stock_sold",
                                  &portfolio::register_stock_sold,
                                  "list_stocks",
                                  &portfolio::list_stocks);

      hft.new_usertype<trader>("trader",
                               sol::call_constructor,
                               sol::factories([](int id, double cash, sp<orderBook> ob) {
                                     return std::make_shared<trader>(id, cash, ob);
                               }),
                               "make_order",
                               &trader::make_order,
                               "update_portfolio",
                               &trader::update_portfolio,
                               "cancel_order",
                               &trader::cancel_order,
                               "do_random_action",
                               &trader::do_random_action,
                               "m_available_cash",
                               &trader::m_available_cash,
                               "m_id",
                               &trader::m_id);

      hft.new_usertype<market>("market",
                               sol::call_constructor,
                               sol::factories([]() { return std::make_shared<market>(); }),
                               "add_trader",
                               &market::add_trader,
                               "add_stock",
                               &market::add_stock,
                               "get_interest_rate",
                               &market::get_interest_rate,
                               "get_stocks",
                               &market::get_stocks,
                               "get_traders",
                               &market::get_traders,
                               "run",
                               &market::run,
                               "reset",
                               &market::reset,
                               "stop",
                               &market::stop,
                               "pause",
                               &market::pause,
                               "resume",
                               &market::resume,
                               "fluctuate",
                               &market::fluctuate,
                               "generate_random_market_event",
                               &market::generate_random_market_event,
                               "execute_order_book",
                               &market::execute_order_book,
                               "apply_market_impact",
                               &market::apply_market_impact);
      SetLogCallBack([=](sv color, sv msg, sv type) {
            std::unique_lock<std::mutex> Lock(logging_mutex);
            if (type == "WARNING") {
                  current_color = glm::vec4(1, 1, 0, 1);
            } else if (type == "ERROR") {
                  current_color = glm::vec4(1, 0, 0, 1);
            } else if (type == "DEBUG") {
                  current_color = glm::vec4(0, 0, 1, 1);
            } else {
                  current_color = glm::vec4(0, 1, 0, 1);
            }
            current_log = msg;
            return 0;
      });
      hft.set_function("GetLogString", [&]() {
            std::unique_lock<std::mutex> Lock(logging_mutex);
            return current_log;
      });
      hft.set_function("GetLogColor", [&]() {
            std::unique_lock<std::mutex> Lock(logging_mutex);
            return current_color;
      });
}

extern "C" DLLEXPORT int luaopen_hftsim(lua_State *L) {
      sol::state_view s(L);
      bind_hft(s);
      return 1;
}