interface Item51 { id: number; name: string; }
type Key51 = string | number;
export function make51(id: number, name: string): Item51 {
    const value: Item51 = { id, name };
    return value as Item51;
}
export const item51: Item51 = make51(51, "item-51");
