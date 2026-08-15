interface Item8 { id: number; name: string; }
type Key8 = string | number;
export function make8(id: number, name: string): Item8 {
    const value: Item8 = { id, name };
    return value as Item8;
}
export const item8: Item8 = make8(8, "item-8");
