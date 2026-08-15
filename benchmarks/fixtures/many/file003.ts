interface Item3 { id: number; name: string; }
type Key3 = string | number;
export function make3(id: number, name: string): Item3 {
    const value: Item3 = { id, name };
    return value as Item3;
}
export const item3: Item3 = make3(3, "item-3");
