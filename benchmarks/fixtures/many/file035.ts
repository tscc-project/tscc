interface Item35 { id: number; name: string; }
type Key35 = string | number;
export function make35(id: number, name: string): Item35 {
    const value: Item35 = { id, name };
    return value as Item35;
}
export const item35: Item35 = make35(35, "item-35");
