interface Item25 { id: number; name: string; }
type Key25 = string | number;
export function make25(id: number, name: string): Item25 {
    const value: Item25 = { id, name };
    return value as Item25;
}
export const item25: Item25 = make25(25, "item-25");
