interface Item47 { id: number; name: string; }
type Key47 = string | number;
export function make47(id: number, name: string): Item47 {
    const value: Item47 = { id, name };
    return value as Item47;
}
export const item47: Item47 = make47(47, "item-47");
