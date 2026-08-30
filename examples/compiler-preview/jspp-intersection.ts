interface Reading { values: [number, number]; }

function total(reading: Reading): number {
    return reading.values[0] + reading.values[1];
}

total({ values: [20, 22] });
