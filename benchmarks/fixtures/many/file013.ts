interface Item13 { id: number; name: string; }
type Key13 = string | number;
export function make13(id: number, name: string): Item13 {
    const value: Item13 = { id, name };
    return value as Item13;
}
export const item13: Item13 = make13(13, "item-13");
