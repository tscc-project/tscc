interface Item42 { id: number; name: string; }
type Key42 = string | number;
export function make42(id: number, name: string): Item42 {
    const value: Item42 = { id, name };
    return value as Item42;
}
export const item42: Item42 = make42(42, "item-42");
