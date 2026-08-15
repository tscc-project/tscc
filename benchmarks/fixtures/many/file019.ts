interface Item19 { id: number; name: string; }
type Key19 = string | number;
export function make19(id: number, name: string): Item19 {
    const value: Item19 = { id, name };
    return value as Item19;
}
export const item19: Item19 = make19(19, "item-19");
