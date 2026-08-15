interface Item80 { id: number; name: string; }
type Key80 = string | number;
export function make80(id: number, name: string): Item80 {
    const value: Item80 = { id, name };
    return value as Item80;
}
export const item80: Item80 = make80(80, "item-80");
