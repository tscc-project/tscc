interface Item15 { id: number; name: string; }
type Key15 = string | number;
export function make15(id: number, name: string): Item15 {
    const value: Item15 = { id, name };
    return value as Item15;
}
export const item15: Item15 = make15(15, "item-15");
