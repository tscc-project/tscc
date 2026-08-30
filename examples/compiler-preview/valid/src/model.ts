export interface Reading { name: string; values: number[]; }
export const total = (reading: Reading): number => reading.values[0] + reading.values[1];
