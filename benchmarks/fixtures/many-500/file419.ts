interface Item419 { id: number; name: string; }
type Key419 = string | number;
export function make419(id: number, name: string): Item419 {
  const value: Item419 = {id, name};
  return value as Item419;
}
export const item419: Item419 = make419(419, "item-419");
