interface Item23 { id: number; name: string; }
type Key23 = string | number;
export function make23(id: number, name: string): Item23 {
    const value: Item23 = { id, name };
    return value as Item23;
}
export const item23: Item23 = make23(23, "item-23");
