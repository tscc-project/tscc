interface Item9 { id: number; name: string; }
type Key9 = string | number;
export function make9(id: number, name: string): Item9 {
    const value: Item9 = { id, name };
    return value as Item9;
}
export const item9: Item9 = make9(9, "item-9");
