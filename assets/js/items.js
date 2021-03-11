var ids = {
    TIRISFAL_SET: 649,
    SPELLFIRE_SET: 552,
    SERPENT_COIL: 30720,
    SILVER_CRESCENT: 29370,
    CHAOTIC_SKYFIRE: 34220,
};

var equip = {
    main_hand: [
        { id: 30095, title: "Fang of the Leviathan", int: 20, sp: 221, crit: 21 },
    ],
    off_hand: [
        { id: 30049, title: "Fathomstone", int: 12, sp: 36, crit: 23 },
    ],
    two_hand: [
        //
    ],
    ranged: [
        { id: 29982, title: "Wand of the Forgotten Star", sp: 23, crit: 14, hit: 11 },
    ],
    head: [
        { id: 30206, itemset: ids.TIRISFAL_SET, title: "Cowl of Tirisfal", int: 36, spi: 24, sp: 55, crit: 24, sockets: ["m", "y"], bonus: { hit: 4 } },
    ],
    neck: [
        { id: 30015, title: "The Sun King's Talisman", int: 16, sp: 41, crit: 24 },
    ],
    shoulder: [
        { id: 30210, itemset: ids.TIRISFAL_SET, title: "Mantle of Tirisfal", int: 24, spi: 24, sp: 40, crit: 17, sockets: ["y", "b"], bonus: { sp: 4 } },
    ],
    back: [
        { id: 29992, title: "Royal Cloak of the Sunstriders", int: 22, sp: 44 },
    ],
    chest: [
        { id: 30196, itemset: ids.TIRISFAL_SET, title: "Robes of Tirisfal", int: 35, spi: 20, sp: 55, crit: 19, sockets: ["y", "y", "b"], bonus: { sp: 5 } },
        { id: 21848, itemset: ids.SPELLFIRE_SET, title: "Spellfire Robe", int: 17, sp_arcane: 72, sp_fire: 72, crit: 28, sockets: ["y", "b"] },
    ],
    wrist: [
        { id: 29918, title: "Mindstorm Wristbands", int: 13, sp: 36, crit: 23 },
    ],
    hands: [
        { id: 29987, title: "Gauntlets of the Sun King", int: 29, spi: 20, sp: 42, crit: 28 },
        { id: 30205, itemset: ids.TIRISFAL_SET, title: "Gloves of Tirisfal", int: 27, spi: 18, sp: 41, crit: 27 },
        { id: 21846, itemset: ids.SPELLFIRE_SET, title: "Spellfire Gloves", int: 10, sp_arcane: 50, sp_fire: 50, crit: 23, sockets: ["y", "b"] },
    ],
    waist: [
        { id: 30038, title: "Belt of Blasting", sp: 50, crit: 30, hit: 23, sockets: ["b", "y"], bonus: { sp: 4 } },
        { id: 21847, itemset: ids.SPELLFIRE_SET, title: "Spellfire Belt", int: 18, sp_arcane: 50, sp_fire: 50, crit: 18, sockets: ["y", "b"] },
    ],
    legs: [
        { id: 30207, itemset: ids.TIRISFAL_SET, title: "Leggings of Tirisfal", int: 36, spi: 26, sp: 54, crit: 17, hit: 26, sockets: ["y"], bonus: { hit: 2 } },
    ],
    feet: [
        { id: 30067, title: "Velvet Boots of the Guardian", int: 21, sp: 49, crit: 24 },
    ],
    finger: [
        { id: 29287, title: "Violet Signet of the Archmage", int: 23, sp: 29, crit: 17 },
        { id: 28753, title: "Ring of Recurrence", int: 19, sp: 32, crit: 19 },
    ],
    trinket: [
        { id: ids.SERPENT_COIL, title: "Serpent-Coil Braid", crit: 30, hit: 12 },
        { id: ids.SILVER_CRESCENT, title: "Icon of the Silver Crescent", sp: 43 },
    ],
};

var gems = [
    { id: ids.CHAOTIC_SKYFIRE, title: "Chaotic Skyfire Diamond", slot: "m", crit: 12 },
    { id: 30551, title: "Infused Fire Opal", slot: "o", int: 4, sp: 6 },
    { id: 33133, title: "Don Julio's Heart", slot: "r", sp: 14, unique: true },
    { id: 28118, title: "Runed Living Ruby", slot: "r", sp: 12, unique: true },
    { id: 24030, title: "Runed Ornate Ruby", slot: "r", sp: 9 },
];

var enchants = {
    head: [
        { id: 29191, title: "Glyph of Power", sp: 22, hit: 14 },
    ],
    shoulder: [
        { id: 28886, title: "Greater Inscription of Discipline (Aldor)", sp: 18, crit: 10 },
    ],
    chest: [
        { id: 27960, title: "Exceptional Stats", int: 6, spi: 6 },
    ],
    wrist: [
        { id: 27917, title: "Spellpower", sp: 15 },
    ],
    weapon: [
        { id: 27891, title: "Sunfire", sp_arcane: 50, sp_fire: 50 },
    ],
    hands: [
        { id: 33997, title: "Major spellpower", sp: 20 },
    ],
    legs: [
        { id: 24274, title: "Runic Spellthread", sp: 35 },
    ],
};

export default { ids, equip, gems, enchants };