interface Item66 { id: number; name: string; }
type Key66 = string | number;
export function make66(id: number, name: string): Item66 {
    const value: Item66 = { id, name };
    return value as Item66;
}
export const item66: Item66 = make66(66, "item-66");
