interface Item4 { id: number; name: string; }
type Key4 = string | number;
export function make4(id: number, name: string): Item4 {
    const value: Item4 = { id, name };
    return value as Item4;
}
export const item4: Item4 = make4(4, "item-4");
