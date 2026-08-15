interface Item6 { id: number; name: string; }
type Key6 = string | number;
export function make6(id: number, name: string): Item6 {
    const value: Item6 = { id, name };
    return value as Item6;
}
export const item6: Item6 = make6(6, "item-6");
