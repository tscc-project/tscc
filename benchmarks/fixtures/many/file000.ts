interface Item0 { id: number; name: string; }
type Key0 = string | number;
export function make0(id: number, name: string): Item0 {
    const value: Item0 = { id, name };
    return value as Item0;
}
export const item0: Item0 = make0(0, "item-0");
