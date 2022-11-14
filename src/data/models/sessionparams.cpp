#include "sessionparams.hpp"

#include <libtorrent/session.hpp>

#include "../statement.hpp"

using porla::Data::Models::SessionParams;

libtorrent::session_params SessionParams::GetLatest(sqlite3 *db)
{
    lt::session_params params;

    Statement::Prepare(db, "SELECT data FROM sessionparams ORDER BY timestamp DESC LIMIT 1")
        .Step(
            [&params](auto const& row)
            {
                params = lt::read_session_params(row.GetBuffer(0));
                return SQLITE_OK;
            });

    return params;
}

void SessionParams::Insert(sqlite3 *db, const libtorrent::session_params &params)
{
    std::vector<char> buf = lt::write_session_params_buf(
        params,
        lt::session::save_dht_state);

    Statement::Prepare(db, "INSERT INTO sessionparams (data) VALUES ($1);")
        .Bind(1, buf)
        .Execute();
}
