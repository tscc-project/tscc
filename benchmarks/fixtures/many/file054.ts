interface Item54 { id: number; name: string; }
type Key54 = string | number;
export function make54(id: number, name: string): Item54 {
    const value: Item54 = { id, name };
    return value as Item54;
}
export const item54: Item54 = make54(54, "item-54");
