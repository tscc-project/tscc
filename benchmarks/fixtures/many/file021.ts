interface Item21 { id: number; name: string; }
type Key21 = string | number;
export function make21(id: number, name: string): Item21 {
    const value: Item21 = { id, name };
    return value as Item21;
}
export const item21: Item21 = make21(21, "item-21");
