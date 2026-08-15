interface Item17 { id: number; name: string; }
type Key17 = string | number;
export function make17(id: number, name: string): Item17 {
    const value: Item17 = { id, name };
    return value as Item17;
}
export const item17: Item17 = make17(17, "item-17");
