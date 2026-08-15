interface Item95 { id: number; name: string; }
type Key95 = string | number;
export function make95(id: number, name: string): Item95 {
    const value: Item95 = { id, name };
    return value as Item95;
}
export const item95: Item95 = make95(95, "item-95");
