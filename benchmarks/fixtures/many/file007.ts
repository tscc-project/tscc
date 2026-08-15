interface Item7 { id: number; name: string; }
type Key7 = string | number;
export function make7(id: number, name: string): Item7 {
    const value: Item7 = { id, name };
    return value as Item7;
}
export const item7: Item7 = make7(7, "item-7");
