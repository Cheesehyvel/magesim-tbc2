/******/ (() => { // webpackBootstrap
var __webpack_exports__ = {};
/*!*********************************!*\
  !*** ./assets/js/sim_worker.js ***!
  \*********************************/
importScripts("./magesim.js");

onmessage = function onmessage(event) {
  var data = event.data;

  var parseTalentsLink = function parseTalentsLink(str) {
    if (str.match(/^[0-9]+$/)) return str;
    var m = str.match(/tbcdb\.com.*mage\&([0-9]+)/i);
    if (m) return m[1];
    return "---";
  };

  if (data.type == "start") {
    var wasm = fetch('./magesim.wasm').then(function (r) {
      return r.arrayBuffer();
    }).then(function (binary) {
      return MageSim({
        wasmBinary: binary
      });
    }).then(function (w) {
      return w.ready;
    }).then(function (m) {
      var config = m.allocConfig();

      for (var key in data.config) {
        if (typeof config[key] != "undefined") config[key] = data.config[key];
      }

      var player = m.allocPlayer(config);
      var stats = JSON.parse(JSON.stringify(player.stats));

      for (var key in data.config.stats) {
        if (stats.hasOwnProperty(key)) stats[key] = data.config.stats[key];
      }

      player.setStats(stats);

      if (data.config.talents) {
        var talents = parseTalentsLink(data.config.talents);
        player.loadTalentsFromString(talents);
      }

      if (data.iterations && data.iterations > 1) var result = m.runSimulations(config, player, data.iterations);else var result = m.runSimulation(config, player);
      m.freePlayer(player);
      m.freeConfig(config);
      postMessage({
        type: "success",
        result: result
      });
    })["catch"](function (e) {
      return console.error(e);
    });
  }
};
/******/ })()
;