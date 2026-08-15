interface Item64 { id: number; name: string; }
type Key64 = string | number;
export function make64(id: number, name: string): Item64 {
    const value: Item64 = { id, name };
    return value as Item64;
}
export const item64: Item64 = make64(64, "item-64");
