interface Item45 { id: number; name: string; }
type Key45 = string | number;
export function make45(id: number, name: string): Item45 {
    const value: Item45 = { id, name };
    return value as Item45;
}
export const item45: Item45 = make45(45, "item-45");
