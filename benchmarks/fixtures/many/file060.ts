interface Item60 { id: number; name: string; }
type Key60 = string | number;
export function make60(id: number, name: string): Item60 {
    const value: Item60 = { id, name };
    return value as Item60;
}
export const item60: Item60 = make60(60, "item-60");
