interface Props73 { value: number; label: string }
export const View73 = (p: Props73) => (
  <section data-i="73">
    <h2>{p.label}</h2>
    {p.value > 73 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
