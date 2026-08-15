interface Item81 { id: number; name: string; }
type Key81 = string | number;
export function make81(id: number, name: string): Item81 {
    const value: Item81 = { id, name };
    return value as Item81;
}
export const item81: Item81 = make81(81, "item-81");
