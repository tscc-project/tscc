interface Item75 { id: number; name: string; }
type Key75 = string | number;
export function make75(id: number, name: string): Item75 {
    const value: Item75 = { id, name };
    return value as Item75;
}
export const item75: Item75 = make75(75, "item-75");
