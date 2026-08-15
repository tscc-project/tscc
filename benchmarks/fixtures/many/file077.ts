interface Item77 { id: number; name: string; }
type Key77 = string | number;
export function make77(id: number, name: string): Item77 {
    const value: Item77 = { id, name };
    return value as Item77;
}
export const item77: Item77 = make77(77, "item-77");
