interface Item68 { id: number; name: string; }
type Key68 = string | number;
export function make68(id: number, name: string): Item68 {
    const value: Item68 = { id, name };
    return value as Item68;
}
export const item68: Item68 = make68(68, "item-68");
