interface Item94 { id: number; name: string; }
type Key94 = string | number;
export function make94(id: number, name: string): Item94 {
    const value: Item94 = { id, name };
    return value as Item94;
}
export const item94: Item94 = make94(94, "item-94");
