interface Item30 { id: number; name: string; }
type Key30 = string | number;
export function make30(id: number, name: string): Item30 {
    const value: Item30 = { id, name };
    return value as Item30;
}
export const item30: Item30 = make30(30, "item-30");
