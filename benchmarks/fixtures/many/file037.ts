interface Item37 { id: number; name: string; }
type Key37 = string | number;
export function make37(id: number, name: string): Item37 {
    const value: Item37 = { id, name };
    return value as Item37;
}
export const item37: Item37 = make37(37, "item-37");
