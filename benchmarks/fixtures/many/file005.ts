interface Item5 { id: number; name: string; }
type Key5 = string | number;
export function make5(id: number, name: string): Item5 {
    const value: Item5 = { id, name };
    return value as Item5;
}
export const item5: Item5 = make5(5, "item-5");
