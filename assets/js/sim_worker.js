importScripts("./magesim.js");

onmessage = (event) => {
    var data = event.data;

    var parseTalentsLink = (str) => {
        if (str.match(/^[0-9\-]+$/))
            return str;

        var m = str.match(/tbc\.wowhead\.com.*mage\/([0-9\-]+)/i);
        if (m)
            return m[1];

        return "---";
    };

    if (data.type == "start") {
        const wasm = fetch("./magesim.wasm", {cache: "no-store"})
        .then(r => r.arrayBuffer())
        .then(binary => MageSim({wasmBinary: binary}))
        .then(w => w.ready)
        .then(m => {
            var config = m.allocConfig();
            for (var key in data.config) {
                if (typeof(config[key]) != "undefined") {
                    if (key.indexOf("_at") > 0 && data.config[key] < 0)
                        config[key] = data.config.duration + data.config[key];
                    else
                        config[key] = data.config[key];
                }
            }
            if (m.RegenRotation.values.hasOwnProperty(data.config.regen_rotation))
                config.regen_rotation = m.RegenRotation.values[data.config.regen_rotation];
            if (m.Trinket.values.hasOwnProperty(data.config.trinket1))
                config.trinket1 = m.Trinket.values[data.config.trinket1];
            if (m.Trinket.values.hasOwnProperty(data.config.trinket2))
                config.trinket2 = m.Trinket.values[data.config.trinket2];
            if (m.MetaGem.values.hasOwnProperty(data.config.meta_gem))
                config.meta_gem = m.MetaGem.values[data.config.meta_gem];
            if (m.Flask.values.hasOwnProperty(data.config.flask))
                config.flask = m.Flask.values[data.config.flask];
            if (m.Elixir.values.hasOwnProperty(data.config.battle_elixir))
                config.battle_elixir = m.Elixir.values[data.config.battle_elixir];
            if (m.Elixir.values.hasOwnProperty(data.config.guardian_elixir))
                config.guardian_elixir = m.Elixir.values[data.config.guardian_elixir];
            if (m.WeaponOil.values.hasOwnProperty(data.config.weapon_oil))
                config.weapon_oil = m.WeaponOil.values[data.config.weapon_oil];
            if (m.Food.values.hasOwnProperty(data.config.food))
                config.food = m.Food.values[data.config.food];
            if (m.Drums.values.hasOwnProperty(data.config.drums))
                config.drums = m.Drums.values[data.config.drums];
            if (m.Potion.values.hasOwnProperty(data.config.potion))
                config.potion = m.Potion.values[data.config.potion];
            if (m.Conjured.values.hasOwnProperty(data.config.conjured))
                config.conjured = m.Conjured.values[data.config.conjured];

            var player = m.allocPlayer(config);
            var stats = JSON.parse(JSON.stringify(player.stats));
            for (var key in data.config.stats) {
                if (stats.hasOwnProperty(key))
                    stats[key] = data.config.stats[key];
            }
            if (m.Race.values.hasOwnProperty(data.config.race))
                player.race = m.Race.values[data.config.race];
            if (m.Race.values.hasOwnProperty(data.config.spec))
                player.spec = m.Race.values[data.config.spec];
            player.setStats(stats);

            if (data.config.talents) {
                var talents = parseTalentsLink(data.config.talents);
                player.loadTalentsFromString(talents);
            }

            if (data.iterations && data.iterations > 1)
                var result = m.runSimulations(config, player, data.iterations);
            else
                var result = m.runSimulation(config, player);

            if (result.log)
                result.log = JSON.parse(result.log);
            if (result.histogram)
                result.histogram = JSON.parse(result.histogram);

            postMessage({
                type: "success",
                result: result
            });
        })
        .catch(e => console.error(e));
    }
}