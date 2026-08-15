interface Item88 { id: number; name: string; }
type Key88 = string | number;
export function make88(id: number, name: string): Item88 {
    const value: Item88 = { id, name };
    return value as Item88;
}
export const item88: Item88 = make88(88, "item-88");
